# Make MAVSDK Release

These are the instructions on how to get a release out the door.

> **Note** The idea is of course to automate this as much as possible!

## MAVSDK part

1. Check if all open pull requests are merged that need to go in.
1. Check that CI on `main` passed.
1. Create tag in `main`:
   ```
   git switch main
   git pull
   git tag vX.Y.Z
   git push origin vX.Y.Z
   ```
1. - Generate changelog using `tools/generate_changelog.py --token TOKEN_FROM_GITHUB --verbose --tag vX.Y.Z`.
   - If it finds old/wrong PRs, wait a bit or create the release from the tag in the [GitHub UI](https://github.com/mavlink/MAVSDK/releases).
   - Once it finds the correct PRs it will check all of them for labels. Make sure all merged PRs have sensible labels (e.g. `enhancement`, `bug`, or `feature`).
   - Copy the changelog text to the [release](https://github.com/mavlink/MAVSDK/releases).
1. Check later if all artifacts have been uploaded correctly to the release.
1. Update the Arch AUR repository. This depends on the AUR maintainter's credentials (currently julianoes).
   - Use the repo: `ssh://aur@aur.archlinux.org/mavsdk.git`.
   - Bump the version in `pkgver=X.Y.Z`.
   - Check PKGBUILD: `namcap PKGBUILD`.
   - Try to make pkg: `makepkg`.
   - Update `.SRCINFO`: `makepkg --printsrcinfo > .SRCINFO`.
   - Commit and push.
1. Update the brew repo. Steps for macOS:
   - `export HOMEBREW_GITHUB_API_TOKEN=<GITHUB API TOKEN>`
   - `brew bump-formula-pr mavsdk --tag=vX.Y.Z --revision=<GIT HASH>`
   - This should then open the browser with the pull request already created.

## MAVSDK-docs part

1. [Generate the docs](../guide/build_docs.md) and update the reference docs.
1. Check or update the examples in the docs.
1. Create a branch off `main` for the release
   - Create the branch
     ```
     git switch main
     git pull
     git switch -c vX.Y
     ```
   - Modify **book.json** in the vX.Y branch to change the value of `github_branch` to match the new branch: `"github_branch": "vX.Y"`
   - Push the branch to the upstream repo
     ```
     git push origin vX.Y
     ```
1. Add the branch to the version checker in main branch [book.json](https://github.com/mavlink/MAVSDK-docs/blob/main/book.json) (see pattern below `versions`).

## Other

1. Post a note on Slack #mavsdk.
