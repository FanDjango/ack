include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "linuxmips" }
}
--
--build_plat_libs {
--	name = "libs",
--	arch = "powerpc",
--	plat = "linuxppc",
--}

installable {
	name = "pkg",
	map = {
		"+tools",
--		"+libs",
		"./include+pkg",
		["$(PLATIND)/linuxmips/boot.o"] = "+boot",
--       ["$(PLATIND)/linuxppc/libsys.a"] = "./libsys+lib",
	}
}

