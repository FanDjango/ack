clibrary {
	name = "lib",
	srcs = {
		"./access.c", "./break.c", "./chmode.c", "./close.c",
		"./create.c", "./filesize.c",
		--"./lock.c",
		"./modtime.c", "./open.c", "./read.c", "./remove.c",
		"./rename.c", "./seek.c", "./stop.c", "./system.c",
		--"./unlock.c",
		"./write.c",
	},
	hdrs = { "./system.h" },
}
