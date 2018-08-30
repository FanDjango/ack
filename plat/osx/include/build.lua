include("plat/build.lua")

headermap = {}
packagemap = {}

local function addheader(h)
	headermap[h] = "plat/osx/include/"..h
	packagemap["$(PLATIND)/osx/include/"..h] = "plat/osx/include/"..h
end

addheader("ack/plat.h")
addheader("ack/signal.h")
addheader("ack/fcntl.h")
addheader("sys/dirent.h")
addheader("sys/mman.h")
addheader("sys/stat.h")
addheader("sys/types.h")
addheader("sys/ioctl.h")

acklibrary {
	name = "headers",
	hdrs = headermap
}

installable {
	name = "pkg",
	map = packagemap
}
