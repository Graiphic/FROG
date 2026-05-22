$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$sidebarPath = Join-Path $repoRoot "_sidebar.md"
$navTreePath = Join-Path $repoRoot "nav-tree.json"
$displayExtensions = @(".md", ".frog", ".py")

$rootSections = @(
    @{ RelativePath = "Expression"; Label = "Expression" },
    @{ RelativePath = "Examples"; Label = "Examples" },
    @{ RelativePath = "Language"; Label = "Language" },
    @{ RelativePath = "IR"; Label = "IR" },
    @{ RelativePath = "Libraries"; Label = "Libraries" },
    @{ RelativePath = "Profiles"; Label = "Profiles" },
    @{ RelativePath = "IDE"; Label = "IDE" },
    @{ RelativePath = "Conformance"; Label = "Conformance" },
    @{ RelativePath = "Implementations/Reference"; Label = "Reference Implementation" },
    @{ RelativePath = "Roadmap"; Label = "Roadmap" },
    @{ RelativePath = "Strategy/Heilmeier"; Label = "Strategy" }
)

$preferredRootDocs = @(
    @{ File = "FROG-Strategy.md"; Label = "FROG Strategy" },
    @{ File = "FROG-Architecture.md"; Label = "FROG Architecture" },
    @{ File = "FROG-Repository-Guide.md"; Label = "FROG Repository Guide" },
    @{ File = "FROG-Project-Status.md"; Label = "FROG Project Status" },
    @{ File = "GOVERNANCE.md"; Label = "Governance" },
    @{ File = "CONTRIBUTING.md"; Label = "Contributing" },
    @{ File = "CLA.md"; Label = "CLA" }
)

$expectedRootOrder = @(
    "Home",
    "Expression",
    "Examples",
    "Language",
    "IR",
    "Libraries",
    "Profiles",
    "IDE",
    "Conformance",
    "Reference Implementation",
    "Roadmap",
    "Strategy",
    "FROG Strategy",
    "FROG Architecture",
    "FROG Repository Guide",
    "FROG Project Status",
    "Governance",
    "Contributing",
    "CLA"
)

$requiredFiles = @(
    ".nojekyll",
    "index.html",
    "404.html",
    "_sidebar.md",
    "nav-tree.json",
    "_navbar.md",
    "Readme.md",
    "FROG-Strategy.md",
    "FROG-Architecture.md",
    "FROG-Repository-Guide.md",
    "FROG-Project-Status.md",
    ".github/scripts/build-pages-navigation.ps1",
    ".github/scripts/verify-pages-navigation.ps1",
    ".github/workflows/refresh-pages-navigation.yml"
)

function Convert-ToRelativeRepoPath {
    param(
        [Parameter(Mandatory = $true)]
        [string]$FullPath
    )

    return $FullPath.Substring($repoRoot.Length + 1).Replace("\", "/")
}

function Convert-ToDocsifyHref {
    param(
        [Parameter(Mandatory = $true)]
        [string]$RelativePath
    )

    if ($RelativePath -eq "Readme.md" -or $RelativePath -eq "README.md") {
        return "/"
    }

    $segments = $RelativePath.Replace("\", "/") -split "/"
    $encodedSegments = foreach ($segment in $segments) {
        [Uri]::EscapeDataString($segment)
    }

    return "/" + ($encodedSegments -join "/")
}

function Test-IsDisplayFile {
    param(
        [Parameter(Mandatory = $true)]
        [System.IO.FileInfo]$File
    )

    return $displayExtensions -contains $File.Extension.ToLowerInvariant()
}

function Get-DisplayableHrefSet {
    $hrefs = New-Object System.Collections.Generic.HashSet[string]([System.StringComparer]::Ordinal)
    $null = $hrefs.Add("/")

    foreach ($section in $rootSections) {
        $sectionPath = Join-Path $repoRoot $section.RelativePath
        if (-not (Test-Path -LiteralPath $sectionPath)) {
            continue
        }

        Get-ChildItem -LiteralPath $sectionPath -Recurse -File |
            Where-Object { Test-IsDisplayFile -File $_ } |
            ForEach-Object {
                $relativePath = Convert-ToRelativeRepoPath -FullPath $_.FullName
                $null = $hrefs.Add((Convert-ToDocsifyHref -RelativePath $relativePath))
            }
    }

    foreach ($rootDoc in $preferredRootDocs) {
        $rootDocPath = Join-Path $repoRoot $rootDoc.File
        if (-not (Test-Path -LiteralPath $rootDocPath)) {
            continue
        }

        $null = $hrefs.Add((Convert-ToDocsifyHref -RelativePath $rootDoc.File))
    }

    return $hrefs
}

function Add-NavPaths {
    param(
        [Parameter(Mandatory = $true)]
        $Node,
        [Parameter(Mandatory = $true)]
        $Paths
    )

    if (-not $Node) {
        return
    }

    if ($Node.path) {
        $null = $Paths.Add([string]$Node.path)
    }

    foreach ($child in @($Node.children)) {
        Add-NavPaths -Node $child -Paths $Paths
    }
}

foreach ($requiredFile in $requiredFiles) {
    $fullPath = Join-Path $repoRoot $requiredFile
    if (-not (Test-Path -LiteralPath $fullPath)) {
        throw "Missing required Pages automation file: $requiredFile"
    }
}

$sidebarText = Get-Content -LiteralPath $sidebarPath -Raw
$navTree = Get-Content -LiteralPath $navTreePath -Raw | ConvertFrom-Json
$workflowText = Get-Content -LiteralPath (Join-Path $repoRoot ".github/workflows/refresh-pages-navigation.yml") -Raw
$indexHtmlText = Get-Content -LiteralPath (Join-Path $repoRoot "index.html") -Raw

foreach ($token in @(
    ".github/scripts/build-pages-navigation.ps1",
    ".github/scripts/verify-pages-navigation.ps1",
    "contents: write",
    "workflow_dispatch:",
    "schedule:",
    "git add -- _sidebar.md nav-tree.json",
    "git commit -m ""chore: refresh pages navigation"""
)) {
    if ($workflowText -notmatch [regex]::Escape($token)) {
        throw "Workflow is missing expected token: $token"
    }
}

foreach ($token in @(
    "--overview-color:",
    "isOverview: true",
    "graiphic-nav-overview-icon",
    "graiphic-nav-overview-label",
    "is-overview"
)) {
    if ($indexHtmlText -notmatch [regex]::Escape($token)) {
        throw "index.html is missing expected overview-navigation token: $token"
    }
}

if ($sidebarText -match [regex]::Escape("#/?id=")) {
    throw "_sidebar.md still contains legacy broken Docsify routes."
}

$displayableHrefs = Get-DisplayableHrefSet
$navPaths = New-Object System.Collections.Generic.HashSet[string]([System.StringComparer]::Ordinal)

foreach ($section in @($navTree.sections)) {
    foreach ($node in @($section.children)) {
        Add-NavPaths -Node $node -Paths $navPaths
    }
}

foreach ($expectedHref in $displayableHrefs) {
    if ($sidebarText -notmatch [regex]::Escape("]($expectedHref)")) {
        throw "_sidebar.md is missing generated path: $expectedHref"
    }

    if (-not $navPaths.Contains($expectedHref)) {
        throw "nav-tree.json is missing generated path: $expectedHref"
    }

    if (@($navTree.searchPaths) -notcontains $expectedHref) {
        throw "nav-tree.json searchPaths is missing generated path: $expectedHref"
    }
}

$actualRootOrder = @()
foreach ($section in @($navTree.sections)) {
    foreach ($node in @($section.children)) {
        $actualRootOrder += [string]$node.title
    }
}

if ($actualRootOrder.Count -lt $expectedRootOrder.Count) {
    throw "nav-tree.json root navigation is missing entries."
}

for ($index = 0; $index -lt $expectedRootOrder.Count; $index++) {
    if ($actualRootOrder[$index] -ne $expectedRootOrder[$index]) {
        throw "nav-tree.json root navigation order mismatch at index $index. Expected '$($expectedRootOrder[$index])', got '$($actualRootOrder[$index])'."
    }
}

Write-Host "Pages navigation automation checks passed."
