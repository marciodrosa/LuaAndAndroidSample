engine = {}

-- Cals the event of the plugin table, if the event is available.
function engine.callplugin(eventname, context)
	if engine.plugin ~= nil then
		local plugincallback = engine.plugin[eventname]
		if plugincallback ~= nil then
			local result = plugincallback(context)
			engine.processresult(result)
		end
	end
end

-- Processes the result returned from the plugin execution.
function engine.processresult(result)
	if result.newfieldvalue ~= nil then
		setvaluetofield(result.newfieldvalue.value, result.newfieldvalue.field)
	end
end
