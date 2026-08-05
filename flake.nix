{
	description = "VoidEngine development environment";
	inputs = {
		nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";
	};

	outputs = { nixpkgs, ... }:

	let 
		system = "x86_64-linux";
		pkgs = import nixpkgs {
			inherit system;
		};
		llvm = pkgs.llvmPackages;
	in
	{
		devShells.${system}.default = pkgs.mkShell {
			name = "voidengine";
			packages = [
				# C++ toolchain
				llvm.clang
				llvm.clang-tools

				# Debuggers
				llvm.lldb
				pkgs.gdb

				# Build systems
				pkgs.cmake
				pkgs.ninja

				pkgs.ccache

				pkgs.git
			];
		};
		CXX = "${llvm.clang}/bin/clang++";
		CMAKE_GENERATOR = "Ninja";
	};
}
