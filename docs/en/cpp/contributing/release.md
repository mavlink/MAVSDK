# Make MAVSDK Release

These are the instructions on how to get a release out the door.

::: info
The idea is of course to automate this as much as possible.
:::

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
1. - Update the version branch (e.g. `v4`) as well to track main.
   - Create the release on GitHub for the pushed tag. Generate the changelog using the GitHub button.
1. Check later if all artifacts have been uploaded correctly to the release.
1. Update the Arch AUR repository. This depends on the AUR maintainer's credentials (currently julianoes).
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

## Documentation part

Documentation is now in-tree under the `docs/` folder in this repository, so it is released together with the code.

1. [Generate the API reference docs](../guide/build_docs.md) and commit any changes to `docs/en/cpp/api_reference/`.
1. Check or update the examples and guides in `docs/en/` as needed.

## Other

1. Post a note on Discord #mavsdk.
