-- This file is an example of plugin.

-- Auxiliar function used by this plugin.
local function findvaluefromfield(context, fieldname)
	for i, v in ipairs(context.fieldsvalues) do
		if v.field == fieldname and v.field ~= nil then
			return v.value
		end
	end
end

-- Returns the plugin table.
return {
	
	-- Called when a field value is changed.
	-- The context table is represented as the following exemple:
	-- context = {
	--		fieldvalue = { -- the current field
	--			field = "identifier",
	--			value = "the value" -- or nil, if it is an empty field
	--		},
	-- 		fieldsvalues = { -- list of all fields
	-- 			{
	-- 				field = "identifier",
	-- 				value = "the value"
	-- 			},
	-- 		}
	-- }
	onfieldvaluechangedbyuser = function(context)
		local text1 = findvaluefromfield(context, "Value one")
		local text2 = findvaluefromfield(context, "Value two")
		local text3 = ""
		if text1 == nil then text3 = "Please type something on field 1."
		elseif text2 == nil then text3 = "Please type something on field 2." end
		if text1 ~= nil and text2 ~= nil then text3 = "V1: '"..text1.."' V2: '"..text2.."'." end
		-- As result, sets a new value to the field "Value three":
		return {
			newfieldvalue = {
				field = "Value three",
				value = text3,
			}
		}
	end

}
