# String Length preserves Cluster fields

Each String leaf is replaced by its byte length. Cluster field identities and names remain in the same order, including when UTF-8 uses several bytes.

Each String leaf is replaced by its byte length. Cluster field identities and names remain in the same order, including when UTF-8 uses several bytes.

Each String leaf is replaced by its byte length. Cluster field identities and names remain in the same order, including when UTF-8 uses several bytes.

Mode: execution. Profile: studio-draft. Capability: frog.studio.string.raw-bytes@1.
Expected:
result (cluster<"accent":i32,"word":i32>) = {"accent":2,"word":4}


[Open the actual FROG document](main.frog).
