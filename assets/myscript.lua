local function findvaluefromfield(context, fieldname)
	for i, v in ipairs(context.fieldsvalues) do
		if v.field == fieldname and v.field ~= nil then
			return v.value
		end
	end
end

return {

	onfieldvaluechangedbyuser = function(context)
		local text1 = findvaluefromfield(context, "Value one")
		local text2 = findvaluefromfield(context, "Value two")
		local text3 = ""
		if text1 == nil then
			text3 = "Please type something on field 1."
		end
		if text2 == nil then
			text3 = "Please type something on field 2."
		end
		if text1 ~= nil and text2 ~= nil then
			text3 = "V1: '"..text1.."' V2: '"..text2.."'."
		end
		setvaluetofield(text3, "Value three")
	end

}
