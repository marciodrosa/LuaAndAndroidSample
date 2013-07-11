--local text1 = getvaluefromfield "Value one"
--local text2 = getvaluefromfield "Value two"
--setvaluetofield("'"..text1.."' and '"..text2.."'.", "Value three")
--return 0

return {
	onfieldvaluechangedbyuser = function(context)
		local text1 = getvaluefromfield "Value one"
		local text2 = getvaluefromfield "Value two"
		setvaluetofield("'"..text1.."' and '"..text2.."'.", "Value three")
	end
}
