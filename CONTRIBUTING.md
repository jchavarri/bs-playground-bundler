## Building

The project requires to have `esy` installed:

```bash
npm install -g esy
```

Running `esy` from project root folder will install dependencies and build.

### Linking locally

To link the project to be used from a playground in local development.

In `bs-playground-bundler` root folder:

- Call `esy dev_release` to create a symlink to the executable file
- `yarn link`

In the playground project:

- `yarn link "@jchavarri/bs-playground-bundler"`
- `yarn add file:../path/to/bs-playground-bundler`

Now you can `yarn run bsembed -o path/to/embeddedlibs.js` from the playground project.
