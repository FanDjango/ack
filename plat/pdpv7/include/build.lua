include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "./"..h
	packagemap["$(PLATIND)/pdpv7/include/"..h] = "./"..h
end

addheader("ack/plat.h")
addheader("sys/select.h")
addheader("sys/types.h")
addheader("termios.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}


