open Js_of_ocaml_compiler;

type options = {
  output_file: string,
  include_dirs: list(string),
};

type return =
  | NoCmisCmjsFound(list(string))
  | Code(Js_of_ocaml_compiler.Code.program);

let readdir = dir =>
  try(Sys.readdir(dir)) {
  | _ => [||]
  };

let is_directory = dir =>
  try(Sys.is_directory(dir)) {
  | _ => false
  };

/** [folderIsEmpty folder] is true, if [folder] contains no files except
 * "." and ".."
 */
let folderIsEmpty = folder => Array.length(readdir(folder)) == 0;

/** [folderContents] returns the paths of all regular files that are
 * contained in [folder]. Each file is a path starting with [folder].
  */
let folderContents = folder => {
  let rec loop = result =>
    fun
    | [f, ...fs] when is_directory(f) =>
      readdir(f)
      |> Array.to_list
      |> List.sort(String.compare)
      |> List.rev
      |> List.map(Filename.concat(f))
      |> List.append(fs)
      |> loop(result)
    | [f, ...fs] => {
        let res =
          switch (Filename.extension(f)) {
          | ".cmi" => [f ++ ":/static/cmis/", ...result]
          | ".cmj" => [f ++ ":/static/cmjs/", ...result]
          | _ => result
          };
        loop(res, fs);
      }
    | [] => result;

  loop([], [folder]);
};

let getBsLibFolders = () => {
  let rec loop = (res, index) => {
    let bsLibFolder =
      Sys.getenv_opt("npm_package_bsembed_" ++ string_of_int(index));
    switch (bsLibFolder) {
    | Some(pkgName) =>
      loop(
        [Sys.getcwd() ++ "/node_modules/" ++ pkgName ++ "/lib/bs", ...res],
        index + 1,
      )
    | None => res
    };
  };
  loop([], 0);
};

let genCode = ({output_file, include_dirs}) => {
  let code = {|
//Provides: caml_create_file_extern
function caml_create_file_extern(name, content) {
  if (!joo_global_object.bsembed_modules)
    joo_global_object.bsembed_modules = {};
  var baseName = name.split("/").pop();
  var extension = baseName.split(".").pop();
  var moduleName = baseName.replace(/\.[^/.]+$/, "");
  if (!joo_global_object.bsembed_modules[moduleName]) {
    joo_global_object.bsembed_modules[moduleName] = {};
  }
  var embeddedModule = joo_global_object.bsembed_modules[moduleName];
  if (extension === "cmi") {
    embeddedModule.cmi = { name: name, content: content };
  } else if (extension === "cmj") {
    embeddedModule.cmj = { name: name, content: content };
  } else {
    joo_global_object.console.warn(
      "Unexpected file extension " + extension + " for file " + name
    );
  }
  if (embeddedModule.cmi && embeddedModule.cmj) {
    joo_global_object.ocaml.load_module(
      embeddedModule.cmi.name,
      embeddedModule.cmi.content,
      embeddedModule.cmj.name.replace("/static/cmis/", ""),
      embeddedModule.cmj.content
    );
  }
  return 0;
}
|};
  let folders = getBsLibFolders();
  let files = folders |> List.map(folderContents) |> List.flatten; // TODO: detect duplicates

  if (List.length(files) == 0) {
    NoCmisCmjsFound(folders);
  } else {
    let fragments = Linker.parse_string(code);
    fragments
    |> List.iter(fr => Linker.load_fragment(~filename="<dummy>", fr));

    let instr =
      PseudoFs.f(
        ~prim=`caml_create_file_extern,
        ~cmis=Stdlib.StringSet.empty,
        ~files,
        ~paths=include_dirs,
      );
    Code(Code.prepend(Code.empty, instr));
  };
};
