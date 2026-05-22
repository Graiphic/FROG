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

$preferredFileOrderByDirectory = @{
    "Expression" = @(
        "Cache.md",
        "Connector.md",
        "Control structures.md",
        "Diagram.md",
        "Front panel.md",
        "Icon.md",
        "IDE preferences.md",
        "Interface.md",
        "Metadata.md",
        "Schema.md",
        "State and cycles.md",
        "Type.md",
        "Widget interaction.md",
        "Widget.md"
    )
}

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

function Convert-ToNodeKey {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Value
    )

    return ($Value -replace "[^A-Za-z0-9/_\.-]", "-").ToLowerInvariant()
}

function Format-DirectoryLabel {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Name
    )

    return $Name.Replace("_", " ")
}

function Format-DisplayFileLabel {
    param(
        [Parameter(Mandatory = $true)]
        [System.IO.FileInfo]$File
    )

    $extension = $File.Extension.ToLowerInvariant()
    if ($extension -eq ".md") {
        return ([System.IO.Path]::GetFileNameWithoutExtension($File.Name)).Replace("_", " ")
    }

    return $File.Name
}

function Test-IsDisplayFile {
    param(
        [Parameter(Mandatory = $true)]
        [System.IO.FileInfo]$File
    )

    return $displayExtensions -contains $File.Extension.ToLowerInvariant()
}

function Test-IsReadmeFile {
    param(
        [Parameter(Mandatory = $true)]
        [System.IO.FileInfo]$File
    )

    return $File.Name -ieq "Readme.md" -or $File.Name -ieq "README.md"
}

function Find-ReadmeFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DirectoryPath
    )

    return Get-ChildItem -LiteralPath $DirectoryPath -File |
        Where-Object { Test-IsReadmeFile -File $_ } |
        Select-Object -First 1
}

function Get-NavigationSortMetadata {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Name
    )

    $normalizedName = $Name.ToLowerInvariant()
    $match = [regex]::Match($normalizedName, '^(?<number>\d+)(?<remainder>.*)$')

    if ($match.Success) {
        return [pscustomobject]@{
            ModeRank = 0
            Number = [int64]$match.Groups["number"].Value
            Remainder = $match.Groups["remainder"].Value
        }
    }

    return [pscustomobject]@{
        ModeRank = 1
        Number = [int64]::MaxValue
        Remainder = $normalizedName
    }
}

function Get-DisplayExtensionRank {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Extension
    )

    switch ($Extension.ToLowerInvariant()) {
        ".md" { return 0 }
        ".frog" { return 1 }
        ".py" { return 2 }
        default { return 9 }
    }
}

function Get-PreferredDisplayFileRank {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DirectoryPath,
        [Parameter(Mandatory = $true)]
        [System.IO.FileInfo]$File
    )

    $relativeDirectory = Convert-ToRelativeRepoPath -FullPath $DirectoryPath
    if (-not $preferredFileOrderByDirectory.ContainsKey($relativeDirectory)) {
        return [int]::MaxValue
    }

    $preferredFiles = $preferredFileOrderByDirectory[$relativeDirectory]
    $rank = [Array]::IndexOf($preferredFiles, $File.Name)
    if ($rank -ge 0) {
        return $rank
    }

    return [int]::MaxValue
}

function Get-DisplayFiles {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DirectoryPath
    )

    return Get-ChildItem -LiteralPath $DirectoryPath -File |
        Where-Object { Test-IsDisplayFile -File $_ } |
        Sort-Object `
            @{ Expression = { Get-PreferredDisplayFileRank -DirectoryPath $DirectoryPath -File $_ } },
            @{ Expression = { Get-DisplayExtensionRank -Extension $_.Extension } },
            @{ Expression = { (Get-NavigationSortMetadata -Name $_.BaseName).ModeRank } },
            @{ Expression = { (Get-NavigationSortMetadata -Name $_.BaseName).Number } },
            @{ Expression = { (Get-NavigationSortMetadata -Name $_.BaseName).Remainder } },
            @{ Expression = { $_.Name.ToLowerInvariant() } }
}

function Test-DirectoryContainsDisplayContent {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DirectoryPath
    )

    return [bool](Get-ChildItem -LiteralPath $DirectoryPath -Recurse -File |
        Where-Object { Test-IsDisplayFile -File $_ } |
        Select-Object -First 1)
}

function Get-OrderedDirectories {
    param(
        [System.IO.DirectoryInfo[]]$Directories
    )

    if (-not $Directories) {
        return @()
    }

    return $Directories | Sort-Object `
        @{ Expression = { (Get-NavigationSortMetadata -Name $_.Name).ModeRank } },
        @{ Expression = { (Get-NavigationSortMetadata -Name $_.Name).Number } },
        @{ Expression = { (Get-NavigationSortMetadata -Name $_.Name).Remainder } },
        @{ Expression = { $_.Name.ToLowerInvariant() } }
}

function Test-UseCombinedNavigationOrder {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DirectoryPath
    )

    $relativePath = Convert-ToRelativeRepoPath -FullPath $DirectoryPath
    return $relativePath -eq "Conformance/invalid"
}

function Get-OrderedNavigationItems {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DirectoryPath
    )

    $items = New-Object System.Collections.Generic.List[object]
    $readme = Find-ReadmeFile -DirectoryPath $DirectoryPath
    $readmeFullPath = if ($readme) { $readme.FullName } else { $null }

    foreach ($file in (Get-ChildItem -LiteralPath $DirectoryPath -File | Where-Object { Test-IsDisplayFile -File $_ })) {
        if ($readmeFullPath -and $file.FullName -eq $readmeFullPath) {
            continue
        }

        $items.Add([pscustomobject]@{
            Kind = "file"
            Name = $file.BaseName
            SortName = $file.Name.ToLowerInvariant()
            TypeRank = 0
            ExtensionRank = Get-DisplayExtensionRank -Extension $file.Extension
            File = $file
            Directory = $null
        })
    }

    foreach ($directory in (Get-ChildItem -LiteralPath $DirectoryPath -Directory | Where-Object { Test-DirectoryContainsDisplayContent -DirectoryPath $_.FullName })) {
        $items.Add([pscustomobject]@{
            Kind = "directory"
            Name = $directory.Name
            SortName = $directory.Name.ToLowerInvariant()
            TypeRank = 1
            ExtensionRank = 9
            File = $null
            Directory = $directory
        })
    }

    return $items | Sort-Object `
        @{ Expression = { (Get-NavigationSortMetadata -Name $_.Name).ModeRank } },
        @{ Expression = { (Get-NavigationSortMetadata -Name $_.Name).Number } },
        @{ Expression = { $_.TypeRank } },
        @{ Expression = { $_.ExtensionRank } },
        @{ Expression = { (Get-NavigationSortMetadata -Name $_.Name).Remainder } },
        @{ Expression = { $_.SortName } }
}

function Add-UniqueSearchPath {
    param(
        [Parameter(Mandatory = $true)]
        $SearchPaths,
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    if (-not $SearchPaths.Contains($Path)) {
        $SearchPaths.Add($Path)
    }
}

function New-FileNavNode {
    param(
        [Parameter(Mandatory = $true)]
        [System.IO.FileInfo]$File,
        [Parameter(Mandatory = $true)]
        $SearchPaths
    )

    $relativePath = Convert-ToRelativeRepoPath -FullPath $File.FullName
    $href = Convert-ToDocsifyHref -RelativePath $relativePath
    Add-UniqueSearchPath -SearchPaths $SearchPaths -Path $href

    return [ordered]@{
        key = Convert-ToNodeKey -Value $relativePath
        title = Format-DisplayFileLabel -File $File
        path = $href
        kind = if ($File.Extension.ToLowerInvariant() -eq ".md") { "page" } else { "code" }
        children = @()
    }
}

function New-DirectoryNavNode {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DirectoryPath,
        $SearchPaths,
        [string]$DisplayLabel,
        [string]$ParentKey = "root"
    )

    $directoryName = Split-Path -Path $DirectoryPath -Leaf
    $sectionLabel = if ($DisplayLabel) { $DisplayLabel } else { Format-DirectoryLabel -Name $directoryName }
    $readme = Find-ReadmeFile -DirectoryPath $DirectoryPath
    $relativeDirectory = Convert-ToRelativeRepoPath -FullPath $DirectoryPath
    $nodeKey = Convert-ToNodeKey -Value ($relativeDirectory + "/")

    $children = New-Object System.Collections.Generic.List[object]
    if (Test-UseCombinedNavigationOrder -DirectoryPath $DirectoryPath) {
        foreach ($item in (Get-OrderedNavigationItems -DirectoryPath $DirectoryPath)) {
            if ($item.Kind -eq "file") {
                $children.Add((New-FileNavNode -File $item.File -SearchPaths $SearchPaths))
                continue
            }

            $children.Add((New-DirectoryNavNode -DirectoryPath $item.Directory.FullName -SearchPaths $SearchPaths -ParentKey $nodeKey))
        }
    } else {
        $displayFiles = Get-DisplayFiles -DirectoryPath $DirectoryPath
        $readmeFullPath = if ($readme) { $readme.FullName } else { $null }

        foreach ($file in $displayFiles) {
            if ($readmeFullPath -and $file.FullName -eq $readmeFullPath) {
                continue
            }

            $children.Add((New-FileNavNode -File $file -SearchPaths $SearchPaths))
        }

        $childDirectories = Get-ChildItem -LiteralPath $DirectoryPath -Directory |
            Where-Object { Test-DirectoryContainsDisplayContent -DirectoryPath $_.FullName }

        foreach ($childDirectory in (Get-OrderedDirectories -Directories $childDirectories)) {
            $children.Add((New-DirectoryNavNode -DirectoryPath $childDirectory.FullName -SearchPaths $SearchPaths -ParentKey $nodeKey))
        }
    }

    $path = $null
    if ($readme) {
        $path = Convert-ToDocsifyHref -RelativePath (Convert-ToRelativeRepoPath -FullPath $readme.FullName)
        Add-UniqueSearchPath -SearchPaths $SearchPaths -Path $path
    }

    return [ordered]@{
        key = $nodeKey
        title = $sectionLabel
        path = $path
        kind = "category"
        children = @($children.ToArray())
    }
}

function Add-DirectorySidebarSection {
    param(
        [Parameter(Mandatory = $true)]
        [string]$DirectoryPath,
        [Parameter(Mandatory = $true)]
        [int]$Depth,
        [string]$DisplayLabel
    )

    $lines = New-Object System.Collections.Generic.List[string]
    $directoryName = Split-Path -Path $DirectoryPath -Leaf
    $indent = "  " * $Depth
    $readme = Find-ReadmeFile -DirectoryPath $DirectoryPath
    $sectionLabel = if ($DisplayLabel) { $DisplayLabel } else { Format-DirectoryLabel -Name $directoryName }

    if ($readme) {
        $href = Convert-ToDocsifyHref -RelativePath (Convert-ToRelativeRepoPath -FullPath $readme.FullName)
        $lines.Add("$indent- [$sectionLabel]($href)")
    } else {
        $lines.Add("$indent- $sectionLabel")
    }

    if (Test-UseCombinedNavigationOrder -DirectoryPath $DirectoryPath) {
        foreach ($item in (Get-OrderedNavigationItems -DirectoryPath $DirectoryPath)) {
            if ($item.Kind -eq "file") {
                $fileLabel = Format-DisplayFileLabel -File $item.File
                $fileHref = Convert-ToDocsifyHref -RelativePath (Convert-ToRelativeRepoPath -FullPath $item.File.FullName)
                $lines.Add("$indent  - [$fileLabel]($fileHref)")
                continue
            }

            $childLines = Add-DirectorySidebarSection -DirectoryPath $item.Directory.FullName -Depth ($Depth + 1)
            foreach ($childLine in $childLines) {
                $lines.Add($childLine)
            }
        }
    } else {
        $displayFiles = Get-DisplayFiles -DirectoryPath $DirectoryPath
        foreach ($file in $displayFiles) {
            if ($readme -and $file.FullName -eq $readme.FullName) {
                continue
            }

            $fileLabel = Format-DisplayFileLabel -File $file
            $fileHref = Convert-ToDocsifyHref -RelativePath (Convert-ToRelativeRepoPath -FullPath $file.FullName)
            $lines.Add("$indent  - [$fileLabel]($fileHref)")
        }

        $childDirectories = Get-ChildItem -LiteralPath $DirectoryPath -Directory |
            Where-Object { Test-DirectoryContainsDisplayContent -DirectoryPath $_.FullName }

        foreach ($childDirectory in (Get-OrderedDirectories -Directories $childDirectories)) {
            $childLines = Add-DirectorySidebarSection -DirectoryPath $childDirectory.FullName -Depth ($Depth + 1)
            foreach ($childLine in $childLines) {
                $lines.Add($childLine)
            }
        }
    }

    return $lines
}

$searchPaths = New-Object System.Collections.Generic.List[string]
Add-UniqueSearchPath -SearchPaths $searchPaths -Path "/"

$sidebarLines = New-Object System.Collections.Generic.List[string]
$sidebarLines.Add("<!-- Auto-generated by .github/scripts/build-pages-navigation.ps1 -->")
$sidebarLines.Add("")
$sidebarLines.Add("- [Home](/)")

$rootNodes = New-Object System.Collections.Generic.List[object]
$rootNodes.Add([ordered]@{
    key = "root/home"
    title = "Home"
    path = "/"
    kind = "page"
    children = @()
})

foreach ($section in $rootSections) {
    $fullPath = Join-Path $repoRoot $section.RelativePath
    if (-not (Test-Path -LiteralPath $fullPath)) {
        continue
    }

    if (-not (Test-DirectoryContainsDisplayContent -DirectoryPath $fullPath)) {
        continue
    }

    $rootNodes.Add((New-DirectoryNavNode -DirectoryPath $fullPath -SearchPaths $searchPaths -DisplayLabel $section.Label))

    $sectionLines = Add-DirectorySidebarSection -DirectoryPath $fullPath -Depth 0 -DisplayLabel $section.Label
    foreach ($sectionLine in $sectionLines) {
        $sidebarLines.Add($sectionLine)
    }
}

foreach ($rootDoc in $preferredRootDocs) {
    $rootDocPath = Join-Path $repoRoot $rootDoc.File
    if (-not (Test-Path -LiteralPath $rootDocPath)) {
        continue
    }

    $href = Convert-ToDocsifyHref -RelativePath $rootDoc.File
    Add-UniqueSearchPath -SearchPaths $searchPaths -Path $href

    $rootNodes.Add([ordered]@{
        key = Convert-ToNodeKey -Value $rootDoc.File
        title = $rootDoc.Label
        path = $href
        kind = "page"
        children = @()
    })

    $sidebarLines.Add(("- [{0}]({1})" -f $rootDoc.Label, $href))
}

$sidebarLines | Set-Content -LiteralPath $sidebarPath -Encoding UTF8

$navTree = [ordered]@{
    sections = @(
        [ordered]@{
            title = ""
            children = @($rootNodes.ToArray())
        }
    )
    searchPaths = @($searchPaths.ToArray())
}

$navTree | ConvertTo-Json -Depth 100 | Set-Content -LiteralPath $navTreePath -Encoding UTF8

Write-Host "Sidebar rebuilt at $sidebarPath"
Write-Host "Navigation manifest rebuilt at $navTreePath"
