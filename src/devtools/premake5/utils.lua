Utils       = {}

-- Convert LUA table into JSON
function Utils.ToJSON( value, indentLevel )
    indentLevel = indentLevel or 0

    -- Value is a string
    if type( value ) == "string" then
        return '"' .. value:gsub("\\", "\\\\"):gsub( '"', '\\"' ) .. '"'
    -- Value is a number or boolean
    elseif type( value ) == "number" or type( value ) == "boolean" then
        return tostring( value )
    -- Value is a table
    elseif type( value ) == "table" then
        local   isArray         = true
        local   currentIdx      = 1
        for valudIdx, _ in pairs( value ) do
            if valudIdx ~= currentIdx then
                isArray = false
                break
            end
            currentIdx = currentIdx + 1
        end

        -- The table is an array
        local result = {}
        if isArray then
            for _, data in ipairs( value ) do
                table.insert( result, Utils.ToJSON( data, indentLevel + 1 ) )
            end
            return "[ " .. table.concat( result, ", " ) .. " ]"
        -- The table is a table
        else
            for key, data in pairs( value ) do
                if type( data ) ~= "function" then
                    local   keyString = '"' .. tostring( key ) .. '"'
                    table.insert( result, string.rep( "\t", indentLevel + 1 ) .. keyString .. ": " .. Utils.ToJSON( data, indentLevel + 1 ) )
                end
            end
            return "{\n" .. table.concat( result, ",\n" ) .. "\n" .. string.rep( "\t", indentLevel ) .. "}"
        end
    
    -- Otherwise it is null
    else
        return "null"
    end
end