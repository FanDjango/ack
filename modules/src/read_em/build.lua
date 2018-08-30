normalrule {
	name = "c_mnem_narg_h",
	ins = {
		"./m_C_mnem_na",
		"util/cmisc+ed",
		"h/em_table",
		"./argtype"
	},
	outleaves = "C_mnem_narg.h",
	commands = {
		"%{ins} > %{outs}"
	}
}
	
normalrule {
	name = "c_mnem_h",
	ins = {
		"./m_C_mnem",
		"util/cmisc+ed",
		"h/em_table",
		"./argtype"
	},
	outleaves = "C_mnem.h",
	commands = {
		"%{ins} > %{outs}"
	}
}
	
local function variant(name, cflags)
	clibrary {
		name = name,
		vars = {
			["+cflags"] = {
				"-DPRIVATE=static",
				"-DEXPORT=",
				"-DNDEBUG",
				"-DCHECKING",
				unpack(cflags)
			},
		},
		srcs = {
			"./EM_vars.c",
			"./read_em.c",
			"./mkcalls.c",
		},
		hdrs = {
			"./em_comp.h",
		},
		deps = {
			"+c_mnem_h",
			"+c_mnem_narg_h",
			"h+emheaders",
			"modules+headers",
			"modules/src/alloc+lib",
			"modules/src/em_code+headers",
			"modules/src/em_data+lib",
			"modules/src/string+lib",
			"modules/src/system+lib",
			"./*.h",
		}
	}
end

variant("lib_ev", {})
variant("lib_kv", { "-DCOMPACT" })
