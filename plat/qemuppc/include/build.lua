include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "./"..h
	packagemap["$(PLATIND)/qemuppc/include/"..h] = "./"..h
end

addheader("ack/plat.h")
addheader("sys/ioctl.h")
addheader("unistd.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}

