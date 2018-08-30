vars.cflags = {
	"-g", "-Og",
}
vars.ackcflags = {
	"-O6"
}
vars.plats = {
	"cpm",
	"linux386",
	"linux68k",
	"linuxppc",
	"osx386",
	"osxppc",
	"qemuppc",
	"pc86",
	"rpi",
}
vars.plats_with_tests = {
	"linux386",
	"linuxppc",
	"qemuppc",
	"pc86",
}

local plat_packages = {}
local test_packages = {}
for _, p in ipairs(vars.plats) do
	plat_packages[#plat_packages+1] = "plat/"..p.."+pkg"
end
for _, p in ipairs(vars.plats_with_tests) do
	test_packages[#test_packages+1] = "plat/"..p.."/tests+tests"
end

installable {
	name = "ack",
	map = {
		"lang/basic/src+pkg",
		"lang/cem/cemcom.ansi+pkg",
		"lang/m2/comp+pkg",
		"lang/pc/comp+pkg",
		"lang/b/compiler+pkg",
		"util/ack+pkg",
		"util/amisc+pkg",
		"util/arch+pkg",
		"util/ego+pkg",
		"util/led+pkg",
		"util/misc+pkg",
		"util/opt+pkg",
		"examples+pkg",
		plat_packages
	},
}

normalrule {
	name = "tests",
	ins = {
		"first/testsummary.sh",
		test_packages
	},
	outleaves = {
		"stamp"
	},
	commands = {
		"%{ins}"
	}
}
