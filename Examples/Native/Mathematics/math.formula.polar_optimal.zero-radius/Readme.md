# Adaptive polar sampling preserves parameter occurrences

Zero radius gives three coincident coordinate pairs. At angles 0,1,2, cos(2) is negative, so +0*cos(2) is IEEE -0; the independent x oracle preserves that sign. Distinct parameter occurrences are retained; the output must not deduplicate them merely because their coordinates agree.

Zero radius gives three coincident coordinate pairs. At angles 0,1,2, cos(2) is negative, so +0*cos(2) is IEEE -0; the independent x oracle preserves that sign. Distinct parameter occurrences are retained; the output must not deduplicate them merely because their coordinates agree.

Zero radius gives three coincident coordinate pairs. At angles 0,1,2, cos(2) is negative, so +0*cos(2) is IEEE -0; the independent x oracle preserves that sign. Distinct parameter occurrences are retained; the output must not deduplicate them merely because their coordinates agree.

Mode: execution. Profile: studio-draft. Capability: frog.studio.formula.evaluation@1; compiled owned-value service.
Expected:
output-x_values (array<f64>) = [0,0,-0]
output-y_values (array<f64>) = [0,0,0]
output-error (i32) = 0


[Open the actual FROG document](main.frog).
