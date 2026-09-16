<h1>Valid Case — Public Structure Families</h1>

This positive conformance case contains the six public FROG v0.1 structure
families in one canonical source document:

- <code>case</code>, including the canonical boolean If/Else form;
- <code>for_loop</code>;
- <code>while_loop</code>;
- <code>event_structure</code>;
- <code>disabled_structure</code>, with retained Enabled and Disabled regions and no selector terminal.
- <code>conditional_disable_structure</code>, with ordered compile-time conditions, one Default region and no runtime selector terminal.

It exercises canonical terminals, stable outer/body graph port identifiers,
owned regions, typed Event Data fields, explicit event descriptors, disabled-code activation, boundary
tunnels, and non-semantic visual authoring metadata. Every structure node MUST
validate against <code>Expression/schema/frog.structure-node.schema.json</code>.

The <code>boundary</code> object remains authoritative for tunnel ids, value types
and directions. <code>structure_tunnels</code> only preserves wall placement.
