# Event Runtime backend contract (explicit lowered versions 1 and 2)

`frog/sdk/event_runtime_contract.hpp` describes a typed handoff of **already
lowered** Event Structure regions. It does not change `.frog` language version
0.1, interpret `studio_bindings`, or extend the flat `ProgramArtifact` codec.
A frontend must validate source semantics and explicitly emit this contract.

- One stable structure ID and distinct region IDs; exactly one Timeout region.
- Each region contains a separately validated `ProgramArtifact`. Only the
  selected region is executed. Unselected regions produce no runtime events.
- Static subscriptions use `(scope, source, kind, filter)` identity, never
  display text. Notify and filter variants are distinct subscriptions.
- Payload schemas are typed; common fields `source:string`, `type:string`,
  and `time:u64` are read-only dispatcher inputs. A field input identifies an
  explicit body control binding. Body outputs cannot overwrite those fields.
- A filter may expose `discardOutput:bool` and named, typed response outputs.
  The host waits for the consumer's result before applying its default action.
  Rejection, timeout, Stop, and execution failure are **not** permission to
  continue the host action. They must be handled explicitly by the host.
- Dynamic registrations name the region and subscriptions. User Events have
  session-local handles and an immutable payload schema; generation validates
  both identity and payload before delivery. Destroy invalidates the source
  handle for new generation and registration, but preserves previously queued
  occurrences and their existing registration. Explicit Unregister removes its
  pending deliveries. Stop invalidates all handles and wakes outstanding waits.

The reference `frog::runtime::EventSession` has a bounded queue (1024 events),
one consumer, and no private UI or detached worker thread. The existing
three-argument `generateUserEvent` retains Normal FIFO delivery. Its named
`UserEventPriority::High` overload inserts before pending Normal occurrences,
after earlier High occurrences; generation order remains stable within each
priority. Static notifications and synchronous filters enter as Normal. High
never preempts an already executing body or changes a filter completion's
identity/deadline. Capacity, Unregister and Stop apply to both priorities.
The provider enum is not a serialized LabVIEW value or an ABI mapping.
Stable order among equal-priority entries is an explicit FROG provider policy.
The cited NI function establishes High-before-pending-Normal delivery, but does
not explicitly establish ordering between two High occurrences. This contract
does not claim exact NI parity for that unspecified order. `waitNext` blocks on
a condition variable: `-1` waits indefinitely, `0` tests immediately, and finite
timeouts accept 1..2147483647 milliseconds. One call executes one region; loop
re-entry is the caller's responsibility, not inferred from repeated UI events.
Filter dispatch enters the same queue and waits for a response with a bounded
deadline. An expired, unexecuted filter is removed rather than run later.
Body evaluation releases the queue lock so Stop and filter deadlines are not
blocked by execution. Results finishing after Stop, replacement, unregistration
or an expired filter deadline are rejected rather than published to the host.
Owners must Stop and join their consuming thread before destroying the session.

Body execution delegates to `ProgramSession`; it does not add another numeric
evaluator. The opt-in event-body profile adds exact string/path/i64/u64 data
transport between equal types. Conversions to/from 64-bit integers, including
signed/unsigned changes, are rejected with a wire-attributed capability diagnostic.
64-bit integer arithmetic remains rejected because the numeric
provider cannot promise exact arithmetic on MSVC. Default capabilities stay
unchanged.

Destroy follows the requested source/occurrence distinction documented by
[NI Destroy User Event](https://www.ni.com/docs/en-US/bundle/labview-api-ref/page/functions/destroy-user-event.html).
Queued payloads are captured by value. The existing version-1 entry points still admit scalar payload
fields only; they do not claim structured payloads, cross-structure broadcasting,
or Studio dynamic-event lowering. Named High/Normal queue
ordering implements the requested priority distinction from
[NI Generate User Event](https://www.ni.com/docs/en-US/bundle/labview-api-ref/page/functions/generate-user-event.html).
No generated occurrence is retained for a later registration.

This contract alone does not claim Studio integration, canonical source/FIR
lowering, While/For loop execution, or the Win32 production of every notification
offered by an editor. Those consumers must have separate integration evidence.

`EventSession::setControlValueAndPost` is a separately explicit signaling
operation. It reuses the normal control validation, then changes the transient
body-bound control and enqueues the supplied captured notification under one
session mutex. A rejected type, route, payload, stopped session or full queue
leaves the control and queue unchanged. A consumer never observes the candidate
update without its accepted occurrence. Existing `setControlValue` remains a
non-signaling update; existing `post` remains explicit occurrence production.

The compiler/host retains responsibility for the actual source identity and
captures OldVal/NewVal by value at commit. The SDK neither guesses those fields
from labels nor rereads a later control value as a captured payload. Reading
the ordinary terminal during body execution still reads the current control;
that is distinct from reading Event Data. This adds no value category, public
artifact schema, generic reference, or composite transport capability.


## Owned captured notifications (version 2)

loadCaptured(EventStructureArtifact, SourceOwner) extends the same executor.
It does not add a provider, polling loop or native producer. Event schema 2
requires an explicit source/run owner, compiled region payload types and actual
body bindings. A schema-2 body carries that exact owner and uses the existing
ProgramSession(true, true) typed transport. Its deferred Event Data bindings
are accepted only when named by the region's real fieldInputs; every value
must be injected from the accepted occurrence before execution. Scalar
schema-1 bodies and the ordinary load/post entry points remain unchanged.

postCaptured(TypedEventPacket) checks the exact owner and compiled
subscription/schema. Immutable records retain their actual ordered field
identities and types. Borrowed references reuse the host's real identity
registry, generation and strict class/type admission; no pointer or display
string replaces an object. Every captured field is validated again immediately
before execution, including fields not connected to an Event Data input.
Deleting or recreating a referenced target between enqueue and wait rejects
the occurrence and does not execute its body or publish its response.

generateUserEventCaptured requires the actual live User Event handle and a
packet with that exact identity and declared typed schema. Raw post and
generic postCaptured cannot bypass this producer. Destroy preserves already
accepted occurrences; Unregister removes its pending deliveries. High/Normal
order remains the documented existing FROG policy. Stop wakes the consumer and
clears registrations and captured storage for that session generation.

The queue keeps the existing 1024-occurrence bound and independently caps
conservatively accounted captured metadata/data at 32 MiB, including executing
occurrences. Refusal does not truncate, evict or mutate another occurrence.
The budget is released on completion, Unregister and Stop. Event payloads and
live references are in-process values; the typed body codec never serializes a
live reference. A host must mint a distinct occurrence owner for each actual
Run, even when the authoritative Source bytes and Source owner epoch remain
unchanged, then capture fresh borrowed references for that owner.

This stage supports notification dispatch and typed User Event dispatch in the
SDK. Structured filters are explicitly rejected: their captured response and
deferred host action protocol still require a separate implementation. Public
Source/openIR lowering, Win32 Mouse/Key producers, panel locking, multiple
subscribers and ordinary loop execution require their own integration evidence.
None of those capabilities is promoted by this SDK contract.


## Nonblocking captured filter decisions

`EventSession::postCapturedFilter(packet, hostActionToken)` admits an actual
compiled filter subscription and captured owner/schema/reference lifetime.
The token must be nonzero and unique across its queued and executing actions.
The same `waitNext` consumer executes the existing ProgramSession body and
returns the in-process `EventDispatchResult::hostActionToken`. The token is
correlation metadata only: it is never a Source identity, persistent reference,
artifact authority, callback or HWND. Raw Post remains notify-only; the old
scalar blocking `dispatchFilter` API retains its existing contract.

`cancelCapturedFilter(token, queuedRemoved)` removes a still-queued occurrence
and releases its retained captured budget. If its body is already executing,
it marks the action abandoned while keeping its data budget until actual body
completion. The consumer then returns Rejected with its token and original
execution trace, but no applicable response or Discard decision. Stop, load
replacement and session teardown abandon the generation and remove all live
action tokens. A host deadline can cancel one action without stopping other
subscriptions. No UI deadline policy is encoded in the SDK.

Every captured payload field is revalidated against the immutable owner and
compiled schema before body execution, including fields not connected to
Event Data. The queue and executing payloads share the existing aggregate
captured-data bound. The host must own the actual pending action, revalidate
its source/run/target on the UI thread, resolve permitted responses before
normal processing, and release that ownership on cancellation, fault or Stop.
The SDK does not perform native default actions or manufacture a successor
notification. Unwired writable fields retain incoming values; absent Discard
output remains false according to the compiled EventCase contract.
