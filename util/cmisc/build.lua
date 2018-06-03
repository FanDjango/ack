cprogram {
	name = "tabgen",
	srcs = { "./tabgen.c" }
}

definerule("tabgen",
	{
		srcs = { type="targets" },
	},
	function(e)
		local symname = basename(filenamesof(e.srcs)[1]):gsub("%.tab$", "")

		return normalrule {
			name = e.name,
			ins = {
				"util/cmisc+tabgen",
				e.srcs
			},
			outleaves = { symname..".c" },
			commands = {
				"%{ins[1]} -f%{ins[2]} > %{outs}"
			}
		}
	end
)

cprogram {
	name = "ed",
	srcs = { "./ed.c" }
}

cprogram {
	name = "bin2c",
	srcs = { "./bin2c.c" }
}

