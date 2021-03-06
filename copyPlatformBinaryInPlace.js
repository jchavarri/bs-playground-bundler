#!/usr/bin/env node

var fs = require("fs");

var arch = process.arch;
var platform = process.platform;

if (arch === "ia32") {
  arch = "x86";
}

if (platform === "win32") {
  platform = "win";
}

var filename = "_release/bsembed-" + platform + "-" + arch + ".exe";
var supported = fs.existsSync(filename);

if (!supported) {
  console.error("bsembed does not support this platform :(");
  console.error("");
  console.error("bsembed comes prepacked as built binaries to avoid large");
  console.error("dependencies at build-time.");
  console.error("");
  console.error("If you want bsembed to support this platform natively,");
  console.error("please open an issue at our repository, linked above. Please");
  console.error("specify that you are on the " + platform + " platform,");
  console.error("on the " + arch + " architecture.");

}

if (!fs.existsSync("bsembed")) {
  copyFileSync(filename, "bsembed");
  fs.chmodSync("bsembed", 0755);
}

if (!fs.existsSync("bsembed.exe")) {
  copyFileSync(filename, "bsembed.exe");
  fs.chmodSync("bsembed.exe", 0755);
}

function copyFileSync(source, dest) {
  if (typeof fs.copyFileSync === "function") {
    fs.copyFileSync(source, dest);
  } else {
    fs.writeFileSync(dest, fs.readFileSync(source));
  }
}
