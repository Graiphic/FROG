# Décoder une mesure dans une trame

Les huit premiers octets codent le DBL 12,5 en ordre réseau. Le dernier octet, 165, appartient au prochain champ.

Les huit premiers octets codent le DBL 12,5 en ordre réseau. Le dernier octet, 165, appartient au prochain champ.

Les huit premiers octets codent le DBL 12,5 en ordre réseau. Le dernier octet, 165, appartient au prochain champ.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
value (f64) = 12.5
remainder (array<u8>) = [165]
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"","status":false}


[Open the actual FROG document](decoder-la-trame.frog).
