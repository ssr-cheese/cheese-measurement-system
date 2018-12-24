clear;

api_accessor = ApiAccessor("classic"); % Please select "half" or "classic" manually.

%amount number of runner
runners_num = 1;

for mouse_index = 1:runners_num
    
    %get player data
    [mouse_name, player_name, org_name] = api_accessor.getDetail(mouse_index);
    
    app = mousetimer(mouse_index,mouse_name, player_name, org_name);
    
    %serialInitialize('COM5',115200,app);
    
    
    %Wait for a player to complete measurement. If it's completed, you must press any key.
    while isvalid(app)
        pause(1);
    end
    
    % If app is closed...
    uploadToWeb(api_accessor, mouse_index);
    
end

%serialEnd;


%app.saveData

%todo
% backup
% defeasance false data
