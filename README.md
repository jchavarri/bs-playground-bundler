# bs-playground-bundler

![](https://github.com/jchavarri/bs-playground-bundler/workflows/bsembed%20pipeline/badge.svg)

Create playground-embeddable files from BuckleScript-compiled libraries.

It includes one binary `bsembed` that can be called with `npx bsembed` or
`yarn run bsembed`.

## Install

```
yarn add -D @jchavarri/bs-playground-bundler
```

or: 

```
npm install --save-dev  @jchavarri/bs-playground-bundler
```

Now you can run `yarn run bsembed -o path/to/embeddedlibs.js` or add
`bsembed -o path/to/embeddedlibs.js` to `package.json` scripts.

## Configuring

`bsembed` works by checking the key `bsembed` in `package.json`. This key should
contain the name of the node modules that we want to embed in the playground.

E.g.:

```json
"bsembed": [
  "reason-react",
  "@glennsl/bs-json"
],
```

`bsembed` will check `node_modules` to look for these modules folders, trying to
gather all `*.cmi` and `*.cmj` files from `/lib/bs`.

`bsembed` takes a param `-o path/to/embeddedlibs.js` to indicate where the
resulting file should be added.

After that, just include this file into the playground web page. Note that the
resulting code assumes that `ocaml.load_modules` is available in the global
namespace, so it is required to load BuckleScript's playground "core" JavaScript
exported file before `bsembed` resulting file.

## Contributing

See [./CONTRIBUTING.md](CONTRIBUTING.md).
