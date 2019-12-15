open Js_of_ocaml_compiler;
open Cmdliner;

let options = {
  let output_file = {
    let doc = "Set output file name to [$(docv)].";
    Arg.(
      required & opt(some(string), None) & info(["o"], ~docv="FILE", ~doc)
    );
  };

  let include_dirs = {
    let doc = "Add [$(docv)] to the list of include directories.";
    Arg.(value & opt_all(string, []) & info(["I"], ~docv="DIR", ~doc));
  };

  Term.(
    const((output_file, include_dirs) => {BsEmbed.output_file, include_dirs})
    $ output_file
    $ include_dirs
  );
};

let info = {
  let doc = "BuckleScript playground dependencies bundler";
  let man = [
    `S("DESCRIPTION"),
    `P(
      "bsembed is a tool for browser playground embeddable files from
      BuckleScript compilation cmi/cmj artifacts.
      The project is forked from jsoo_fs: a tool for embedding files in a
      Js_of_ocaml pseudo filesystem.",
    ),
    `S("BUGS"),
    `P(
      "Bugs are tracked on github at $(i,https://github.com/jchavarri/bs-playground-bundler/issues).",
    ),
  ];

  let version =
    switch (Compiler_version.git_version) {
    | "" => Compiler_version.s
    | v => Printf.sprintf("%s+git-%s", Compiler_version.s, v)
    };

  Term.info("bsembed", ~doc, ~man, ~version);
};

let f = ({BsEmbed.output_file} as args) => {
  Vendor.gen_file(
    output_file,
    chan => {
      let pfs_fmt = Pretty_print.to_out_channel(chan);
      let code = BsEmbed.genCode(args);
      switch (code) {
      | Code(c) =>
        Driver.f(
          ~standalone=true,
          ~global=`Auto,
          pfs_fmt,
          Parse_bytecode.Debug.create(~toplevel=false, false),
          c,
        )
      | NoCmisCmjsFound(folders) =>
        Printf.fprintf(
          stdout,
          "No cmis/cmjs files found.\n\nTried looking in folders:\n%s\n\nMake sure you have:
  1. Added a key `bsembed` to `package.json` (e.g. `\"bsembed\": [\"reason-react\"]`)
  2. Compiled your code with `bsb` before running `bsembed`\n",
          String.concat("\n", folders),
        )
      };
    },
  );
};

let main = (Cmdliner.Term.(const(f) $ options), info);

{
  Timer.init(Sys.time);
  try(
    Cmdliner.Term.eval(
      ~catch=false,
      ~argv=Vendor.normalize_argv(~warn_=true, Sys.argv),
      main,
    )
  ) {
  | (Match_failure(_) | Assert_failure(_) | Not_found) as exc =>
    let backtrace = Printexc.get_backtrace();
    Format.eprintf(
      "%s: You found a bug. Please report it at https://github.com/jchavarri/bs-playground-bundler/issues :@.",
      Sys.argv[0],
    );
    Format.eprintf("Error: %s@.", Printexc.to_string(exc));
    prerr_string(backtrace);
    exit(1);
  | Failure(s) =>
    Format.eprintf("%s: Error: %s@.", Sys.argv[0], s);
    exit(1);
  | exc =>
    let backtrace = Printexc.get_backtrace();
    Format.eprintf("%s: Error: %s@.", Sys.argv[0], Printexc.to_string(exc));
    prerr_string(backtrace);
    exit(1);
  };
};
