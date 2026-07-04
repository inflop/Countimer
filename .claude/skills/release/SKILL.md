---
name: release
description: Use when publishing a new library version — bumping the version, tagging, creating a GitHub release — or when asked what remains before a release can be tagged.
---

# Release a new version

## Overview

The Arduino Library Manager indexes this library **by git tags**, so a wrong tag is
effectively immutable — never delete or move a pushed tag; fix mistakes with a new
patch release. The version lives in two places that must match: `library.properties`
(`version=`) and the tag name. Tag format is bare `x.y.z` (**no `v` prefix** — must
match the existing `1.0.0` tag).

## Preconditions — verify each, in order

1. **Everything merged and clean.** `git status` shows a clean tree; the release commit
   is on `master` (feature work happens on `dev`, releases are tagged on `master`).
2. **Version bumped.** `version=` in `library.properties` equals the intended `x.y.z`
   (SemVer: breaking = major, new API = minor, fixes = patch).
3. **CHANGELOG ready.** `CHANGELOG.md` has a `## [x.y.z] - YYYY-MM-DD` section with the
   release date and content matching what is actually being shipped.
4. **Code verified.** Run the check-api-sync skill, then the verify-examples skill.
5. **CI green** on the release commit: `gh run list --branch master --limit 3`.
   An **empty** list also fails this check — it means CI has never run on that commit
   (e.g. the workflow was never pushed); wait for a green run, don't assume one.

Any precondition failing → stop and report what's missing; do not tag.

## Tag and publish

```sh
git tag x.y.z                 # on the release commit on master
git push origin x.y.z
gh release create x.y.z --title "x.y.z" --notes "<the x.y.z section from CHANGELOG.md>"
```

The Library Manager picks up the new tag automatically (the repo is registered in its
index; `arduino-lint` runs in CI with `library-manager: update` to guard compliance).

## Common mistakes

| Mistake | Consequence / fix |
|---|---|
| Tag `v1.2.0` instead of `1.2.0` | Inconsistent with `1.0.0`; Library Manager treats it as a different scheme. Use bare `x.y.z`. |
| Tagging with `library.properties` still on the old version | Library Manager publishes the tag with the wrong version string. Bump first, tag second. |
| Deleting/re-pushing a bad tag | The index may have already picked it up. Publish a new patch version instead. |
| Tagging on `dev` | The tagged tree may differ from `master`. Merge first, tag the merge commit. |
