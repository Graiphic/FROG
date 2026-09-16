# Event Runtime backend contract (opt-in, version 1)

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
  both identity and payload before delivery. Unregister/destroy removes pending
  deliveries. Stop invalidates all handles and wakes outstanding waits.

The reference `frog::runtime::EventSession` has a bounded FIFO (1024 events),
one consumer, and no private UI or detached worker thread. `waitNext` blocks on
a condition variable: `-1` waits indefinitely, `0` tests immediately, and finite
timeouts accept 1..2147483647 milliseconds. One call executes one region; loop
re-entry is the caller's responsibility, not inferred from repeated UI events.
Filter dispatch enters the same FIFO and waits for a response with a bounded
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

This contract alone does not claim Studio integration, canonical source/FIR
lowering, While/For loop execution, or the Win32 production of every notification
offered by an editor. Those consumers must have separate integration evidence.
