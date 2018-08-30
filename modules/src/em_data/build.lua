local generated = normalrule {
	name = "generated",
	ins = {
		"./new_table",
		"util/cmisc+ed",
		"h/em_table", -- relative to root, which is a bit evil
	},
	outleaves = {
		"em_flag.c",
		"em_pseu.c",
		"em_mnem.c",
		"em_spec.h",
		"em_pseu.h",
		"em_mnem.h",
	},
	deps = {
		"h+emheaders"
	},
	commands = {
		"%{ins[1]} %{ins[2]} %{ins[3]} %{dir} %{dir}"
	}
}

clibrary {
	name = "lib",
	srcs = concat(
		"./em_ptyp.c",
		matching(filenamesof(generated), "%.c$")
	),
	hdrs = {
		"+generated" -- so we export the H files
	},
	deps = {
		"+generated", -- so we can see the H files
		"h+emheaders"
	}
}
