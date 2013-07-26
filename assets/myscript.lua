-- This file is an example of plugin.

-- Returns the plugin table.
return {
	
	-- Called when a field value is changed.
	-- The context table is represented as the following exemple:
	-- context = {
	--		fieldvalue = { -- the current field
	--			field = "identifier",
	--			value = "the value" -- or nil, if it is an empty field
	--		},
	-- 		fieldsvalues = { -- hash of all fields
	-- 			"identifier" = {
	-- 				field = "identifier",
	-- 				value = "the value"
	-- 			},
	-- 		}
	-- }
	onfieldvaluechangedbyuser = function(context)
		local text1 = context.fieldsvalues["Field one"].value
		local text2 = context.fieldsvalues["Field two"].value
		local text3 = ""
		if text1 == nil then
			text3 = "Please type something on field 1."
		elseif text2 == nil then
			text3 = "Please type something on field 2."
		end
		if text1 ~= nil and text2 ~= nil then
			text3 = "V1: '"..text1.."' V2: '"..text2.."'."
		end
		-- As result, sets a new value to the field "Field three":
		return {
			newfieldvalue = {
				field = "Field three",
				value = text3,
			}
		}
	end

}
