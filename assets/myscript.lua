-- This file is an example of plugin.

-- Returns the plugin table.
return {

	-- Called when a field value is changed.
	-- The context table is represented as the following exemple:
	-- context = {
	--		field = { -- the current field
	--			name = "name",
	--			value = "the value" -- or nil, if it is an empty field
	--		},
	-- 		fields = { -- hash of all fields
	-- 			"name" = {
	-- 				name = "name",
	-- 				value = "the value"
	-- 			},
	-- 		}
	-- }
	onfieldvaluechangedbyuser = function(context)
		local op1 = tonumber(context.fields["Field one"].value) or 0
		local op2 = tonumber(context.fields["Field two"].value) or 0
		return {
			newfieldvalue = {
				field = "Field three",
				value = op1 + op2,
			}
		}
	end
}
