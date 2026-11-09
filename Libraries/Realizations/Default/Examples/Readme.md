<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization Examples</h1>

<p align="center">
  <strong>Normative example-corridor posture for the official <code>Default</code> widget realization family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-directory-exists">2. Why this Directory Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#what-an-example-must-prove">4. What an Example Must Prove</a></li>
  <li><a href="#relationship-with-other-default-family-documents">5. Relationship with Other Default-Family Documents</a></li>
  <li><a href="#required-example-layers">6. Required Example Layers</a></li>
  <li><a href="#example-directory-posture">7. Example Directory Posture</a></li>
  <li><a href="#button-example-posture">8. Button Example Posture</a></li>
  <li><a href="#variant-aware-and-skin-aware-example-posture">9. Variant-Aware and Skin-Aware Example Posture</a></li>
  <li><a href="#portability-posture">10. Portability Posture</a></li>
  <li><a href="#validation-posture">11. Validation Posture</a></li>
  <li><a href="#anti-patterns">12. Anti-Patterns</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory defines the normative example-corridor posture for the official <code>Default</code> widget realization family.
</p>

<p>
Its purpose is to ensure that published examples reinforce the realization architecture rather than quietly replacing it with undocumented conventions.
</p>

<p>
An example in this directory is not merely illustrative prose.
It is part of the inspectable publication corridor that links:
</p>

<ul>
  <li>realization doctrine,</li>
  <li>machine-readable realization publication,</li>
  <li>concrete asset-tree organization,</li>
  <li>portable runtime interpretation.</li>
</ul>

<p>
Examples in this directory therefore serve as publication evidence.
They show how doctrinal rules, machine-readable records, and concrete resources align in a form that future runtimes and future contributors can inspect directly.
</p>

<p>
This corridor is especially important for the current normalization direction of the default family because the family now needs to show not only shared realization doctrine, but also bounded specialization through compatible realization variants and compatible skins without allowing those embodiment choices to drift into hidden class splits.
</p>

<hr/>

<h2 id="why-this-directory-exists">2. Why this Directory Exists</h2>

<p>
A realization family may look coherent at the doctrinal level while still remaining ambiguous at the publication level.
That ambiguity usually appears in the examples first.
</p>

<p>
Without a normalized example corridor:
</p>

<ul>
  <li>machine-readable packages drift away from the doctrinal wording,</li>
  <li>asset trees become private conventions instead of inspectable publication artifacts,</li>
  <li>dynamic public text may accidentally collapse into SVG ownership,</li>
  <li>runtime implementations begin to define the real behavior of the system by convention,</li>
  <li>future contributors cannot tell whether a pattern is normative, optional, variant-specific, skin-specific, or accidental.</li>
</ul>

<p>
This directory exists to prevent that drift.
</p>

<p>
It also exists to ensure that compatible embodiment choice remains explicit.
If the family supports a checkbox-like and a switch-like boolean embodiment, or several skins of the same button corridor, the example layer must make that specialization legible without pretending that each visual specialization is a distinct widget class.
</p>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>the role of examples in the <code>Default</code> realization family,</li>
  <li>the minimum publication layers that a realization example should show,</li>
  <li>how example prose, machine-readable JSON, and asset trees should align,</li>
  <li>the portability posture expected from example material,</li>
  <li>how compatible realization variants should be documented when they exist,</li>
  <li>how compatible skin specializations should be documented when they exist.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>widget class law,</li>
  <li>generic realization doctrine,</li>
  <li>the full generic <code>.wfrog</code> specification,</li>
  <li>one mandatory runtime implementation,</li>
  <li>the visual design details of every shipped SVG resource.</li>
</ul>

<hr/>

<h2 id="what-an-example-must-prove">4. What an Example Must Prove</h2>

<p>
A published example in this directory SHOULD prove at least the following:
</p>

<ul>
  <li>which standardized widget class is being targeted,</li>
  <li>which official realization family is being published,</li>
  <li>which parts are realized as visual resource layers,</li>
  <li>which parts are realized through placement-support resources such as anchors or text regions,</li>
  <li>which states are published explicitly,</li>
  <li>which fallbacks are published explicitly,</li>
  <li>how the asset tree corresponds to the machine-readable resource records,</li>
  <li>how a runtime can implement the realization without becoming the semantic owner of the widget.</li>
</ul>

<p>
When a realization family supports compatible embodiment variants, the example SHOULD also make clear:
</p>

<ul>
  <li>whether the example is family-generic or variant-specific,</li>
  <li>which parts and states remain shared across variants,</li>
  <li>which resources or bindings are specialized by variant,</li>
  <li>that the variant does not silently introduce a new widget class.</li>
</ul>

<p>
When a realization family supports compatible skins, the example SHOULD also make clear:
</p>

<ul>
  <li>whether the example is skin-generic or skin-specific,</li>
  <li>which embodiment surfaces remain structurally shared across skins,</li>
  <li>which resources, token maps, or state maps are specialized by skin,</li>
  <li>that the skin does not silently introduce a new class contract.</li>
</ul>

<p>
An example that does not make these boundaries legible is incomplete even if its JSON is syntactically valid.
</p>

<hr/>

<h2 id="relationship-with-other-default-family-documents">5. Relationship with Other Default-Family Documents</h2>

<p>
The example corridor is subordinate to the architectural and doctrinal layers above it.
</p>

<p>
In particular:
</p>

<ul>
  <li><code>Libraries/Realizations/Default/Readme.md</code> defines the family-level posture,</li>
  <li><code>Libraries/Realizations/Default/Package.md</code> defines the machine-readable publication posture,</li>
  <li><code>Libraries/Realizations/Default/Resource model.md</code> defines how realization resources are described,</li>
  <li><code>Libraries/Realizations/Default/State mapping.md</code> defines how state-sensitive realization resources are mapped,</li>
  <li><code>Libraries/Realizations/Default/Assets/Readme.md</code> defines the asset-tree posture,</li>
  <li><code>Libraries/Realizations/Default/Assets/Naming.md</code> defines the naming posture.</li>
</ul>

<p>
Examples in this directory must align with those documents.
They must not silently introduce a second architecture.
</p>

<p>
More specifically:
</p>

<ul>
  <li>examples MUST NOT assign semantic ownership of dynamic public text to asset files when the doctrinal posture assigns that ownership to class law,</li>
  <li>examples MUST NOT collapse <code>state_maps</code> and <code>part_bindings</code> into one undocumented mechanism,</li>
  <li>examples MUST NOT use runtime-specific shortcuts as if they were the normative publication corridor,</li>
  <li>examples MUST NOT turn variant choice into an implicit class split,</li>
  <li>examples MUST NOT turn skin choice into an implicit semantic split.</li>
</ul>

<hr/>

<h2 id="required-example-layers">6. Required Example Layers</h2>

<p>
A coherent realization example SHOULD include, either directly or through clearly paired files:
</p>

<ul>
  <li>a prose explanation of the example and its architectural intent,</li>
  <li>a machine-readable <code>.wfrog</code> realization package example,</li>
  <li>a simulated or concrete asset tree,</li>
  <li>enough resource naming and placement information to show how dynamic public surfaces are realized portably.</li>
</ul>

<p>
For small examples, these layers may be split across multiple files in the same example corridor.
The important point is that the files reinforce one another rather than contradicting one another.
</p>

<p>
The preferred minimum publication stack is therefore:
</p>

<ul>
  <li><strong>example prose</strong> — explains what the example demonstrates and what architectural rule it is meant to prove,</li>
  <li><strong>machine-readable package</strong> — publishes realization records, resources, state maps, part bindings, and variant or skin specializations when relevant,</li>
  <li><strong>asset subtree</strong> — shows how concrete realization resources are organized, named, and scoped,</li>
  <li><strong>placement-support artifacts</strong> — shows how anchors or text regions are used when a host renders dynamic public surfaces.</li>
</ul>

<p>
When skin-aware examples exist, the preferred publication stack MAY also include:
</p>

<ul>
  <li><strong>style-token artifacts</strong> — shows how bounded style-token specialization is published without changing class meaning.</li>
</ul>

<hr/>

<h2 id="example-directory-posture">7. Example Directory Posture</h2>

<p>
The recommended posture for this directory is:
</p>

<pre><code>Libraries/Realizations/Default/Examples/
  Readme.md
  Button default realization.wfrog.md
  Button package and assets example.md
  ButtonPackage/
    default_button_realization.wfrog.json
    assets/
      ...
</code></pre>

<p>
In this posture:
</p>

<ul>
  <li>the example prose explains what is being demonstrated,</li>
  <li>the JSON file shows the machine-readable publication,</li>
  <li>the asset subtree shows how the resources are concretely organized,</li>
  <li>the three layers remain inspectable as one coherent publication unit.</li>
</ul>

<p>
A larger family example corridor MAY later contain several subdirectories, one per standardized widget family or one per realization target.
That expansion remains acceptable only if the architectural split stays visible and the example names remain stable and inspectable.
</p>

<p>
When compatible variants or skins are published, the directory posture SHOULD also make the specialization readable from filenames and subtree structure rather than hiding it only in runtime code.
</p>

<hr/>

<h2 id="button-example-posture">8. Button Example Posture</h2>

<p>
The button example is the first normative reference pattern for this directory.
It is intentionally small, but it must already show the most important separation in the realization corridor.
</p>

<p>
For the standard default button example:
</p>

<ul>
  <li><code>face</code> is a state-sensitive visual part and therefore uses state-mapped visual resources,</li>
  <li><code>frame</code> may be published as an optional visual layer when the realization family exposes it,</li>
  <li><code>label</code> is a dynamic semantic public surface and is therefore preferably published through a placement-support resource such as <code>anchors/label.json</code> or a text-region resource rather than through baked final label SVG files,</li>
  <li>the runtime renders live <code>label.text</code> into the published placement surface.</li>
</ul>

<p>
This is the key example posture that prevents dynamic public text from being redefined by asset ownership.
</p>

<p>
The button example should therefore remain the first reference pattern reused across the rest of the family:
</p>

<ul>
  <li>visual state is shown through <code>state_maps</code>,</li>
  <li>stable structural correspondence is shown through <code>part_bindings</code>,</li>
  <li>dynamic text is shown through placement publication rather than asset-owned semantic text.</li>
</ul>

<p>
Because button realization is also the cleanest first place to show bounded skinning, it is a natural candidate for the first example corridor that later demonstrates one compatible variant and one compatible skin without changing the button class contract.
</p>

<hr/>

<h2 id="variant-aware-and-skin-aware-example-posture">9. Variant-Aware and Skin-Aware Example Posture</h2>

<p>
When the default family allows several compatible realization variants for the same standardized class, the example corridor SHOULD make that variant posture explicit.
</p>

<p>
The preferred rule is:
</p>

<ul>
  <li>a family-generic example proves the shared realization contract of the class,</li>
  <li>a variant-specific example proves how one compatible embodiment specializes that contract without changing class meaning.</li>
</ul>

<p>
For example, a boolean corridor may later contain:
</p>

<ul>
  <li>a family-generic boolean example proving the common boolean realization contract,</li>
  <li>a checkbox-like example proving one compatible embodiment,</li>
  <li>a switch-like example proving another compatible embodiment.</li>
</ul>

<p>
In such cases, the example must make clear:
</p>

<ul>
  <li>which state vocabulary remains shared,</li>
  <li>which parts remain shared,</li>
  <li>which bindings or resources are variant-specific,</li>
  <li>that the example remains inside one class contract unless another class is explicitly published elsewhere.</li>
</ul>

<p>
A variant-specific example must not silently imply:
</p>

<ul>
  <li>a new intrinsic class,</li>
  <li>a new mandatory public part model,</li>
  <li>a new mandatory public method or event surface,</li>
  <li>a runtime-private interpretation rule that only one implementation understands.</li>
</ul>

<p>
When the default family allows several compatible skins for the same realization corridor, the example corridor SHOULD make that skin posture explicit as well.
</p>

<p>
The preferred rule is:
</p>

<ul>
  <li>a skin-generic example proves the shared realization contract and the shared structural mapping,</li>
  <li>a skin-specific example proves how one compatible skin specializes resources or style-token posture without changing widget semantics.</li>
</ul>

<p>
A skin-specific example should therefore make clear:
</p>

<ul>
  <li>which structural bindings remain shared,</li>
  <li>which resource groups are specialized by skin,</li>
  <li>which token maps or color defaults are specialized by skin,</li>
  <li>that the skin remains a realization-side customization rather than a class split.</li>
</ul>

<hr/>

<h2 id="portability-posture">10. Portability Posture</h2>

<p>
Examples in this directory must remain portable across runtime families.
That portability requirement applies especially to:
</p>

<ul>
  <li>Python runtimes,</li>
  <li>Rust runtimes,</li>
  <li>C and C++ runtimes.</li>
</ul>

<p>
Portability does not require those runtimes to use identical host UI toolkits or identical rendering pipelines.
It requires them to be able to consume the same published realization contract without one implementation becoming the hidden semantic source of truth.
</p>

<p>
Accordingly, examples SHOULD publish:
</p>

<ul>
  <li>inspectable resource identities,</li>
  <li>inspectable placement resources,</li>
  <li>explicit state mappings,</li>
  <li>explicit fallbacks,</li>
  <li>clear separation between semantic public surfaces and visual assets.</li>
</ul>

<p>
When variant-specific examples exist, portability also requires that different runtimes can recognize:
</p>

<ul>
  <li>the same base class contract,</li>
  <li>the same realization-family rules,</li>
  <li>the same variant identifier or equivalent specialization marker,</li>
  <li>the same fallback posture when one variant cannot be realized exactly.</li>
</ul>

<p>
When skin-specific examples exist, portability also requires that different runtimes can recognize:
</p>

<ul>
  <li>the same base class contract,</li>
  <li>the same shared part model and state vocabulary,</li>
  <li>the same skin identifier or equivalent specialization marker,</li>
  <li>the same bounded fallback posture when one skin-specific resource group is unavailable.</li>
</ul>

<hr/>

<h2 id="validation-posture">11. Validation Posture</h2>

<p>
Reviewers and validators SHOULD be able to check at least the following from the example corridor:
</p>

<ul>
  <li>the prose example does not contradict the machine-readable package,</li>
  <li>the machine-readable package does not contradict the asset-tree shape,</li>
  <li>resource paths correspond to the published resource records,</li>
  <li>dynamic public text is not silently reassigned to state-specific SVG ownership when the realization contract publishes a placement surface instead,</li>
  <li>fallback posture remains explicit rather than hidden in one runtime loader.</li>
</ul>

<p>
When variant-specific examples exist, validators SHOULD also be able to check:
</p>

<ul>
  <li>the example identifies its variant posture explicitly,</li>
  <li>shared class semantics are not rewritten at the variant layer,</li>
  <li>variant-specific resources and bindings remain inspectable,</li>
  <li>the example does not silently create a new class split by embodiment alone.</li>
</ul>

<p>
When skin-specific examples exist, validators SHOULD also be able to check:
</p>

<ul>
  <li>the example identifies its skin posture explicitly,</li>
  <li>skin-specialized resources remain bounded to realization publication,</li>
  <li>the example does not silently transfer semantic ownership into style assets or token maps,</li>
  <li>the example does not silently create a semantic split by visual customization alone.</li>
</ul>

<hr/>

<h2 id="anti-patterns">12. Anti-Patterns</h2>

<p>
The following example-corridor anti-patterns SHOULD be avoided:
</p>

<ul>
  <li>an example JSON package that contradicts the example asset tree,</li>
  <li>an example asset tree that reintroduces runtime-private assumptions not present in the published realization,</li>
  <li>publishing prose that says a public text surface is dynamic while the example assets still imply that final text is owned by state-specific SVG files,</li>
  <li>treating examples as disposable illustrations rather than as inspectable publication guidance,</li>
  <li>letting one runtime sample implementation become the only place where the actual realization logic can be understood,</li>
  <li>publishing a variant-specific embodiment as if it were automatically a distinct widget class,</li>
  <li>publishing a skin-specific specialization as if it were automatically a distinct class contract,</li>
  <li>using example naming that hides whether the example is family-generic, variant-specific, or skin-specific.</li>
</ul>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
The <code>Examples/</code> corridor of the default realization family exists to keep the publication chain explicit from doctrine to machine-readable package to concrete assets.
</p>

<p>
A correct example in this directory:
</p>

<ul>
  <li>aligns with the doctrinal documents above it,</li>
  <li>aligns with the machine-readable realization package it accompanies,</li>
  <li>aligns with the concrete asset-tree posture,</li>
  <li>helps future runtimes implement the realization portably without becoming the owner of widget semantics.</li>
</ul>

<p>
Its preferred posture is:
</p>

<ul>
  <li>family-level doctrine above,</li>
  <li>machine-readable realization publication in the middle,</li>
  <li>concrete assets below,</li>
  <li>examples proving the whole chain coherently.</li>
</ul>

<p>
This corridor must now also be able to prove a second rule clearly:
compatible realization variants and compatible skins are realization-side specialization mechanisms, not automatic class splits.
</p>

<p>
The next most coherent file to handle after this one is:
</p>

<ul>
  <li><code>Libraries/Realizations/Default/Examples/Button default realization.wfrog.md</code></li>
</ul>

<p>
That file is the natural next step because this <code>Readme.md</code> now defines the example-corridor rules, and the button example is the first normative proof case for the full doctrine-to-package-to-assets chain, including the first clean opportunity to show bounded variant and skin posture in a concrete publication example.
</p>
