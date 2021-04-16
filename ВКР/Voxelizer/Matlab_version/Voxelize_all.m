dirData = dir('73/*.stl');
for index = 1:57
    filename1 = convertStringsToChars(string(dirData(index).folder) + '\' + string(dirData(index).name));
    [OUTPUTgrid] = VOXELISE(32,32,32,filename1,'xyz');
    filename = '73_mat/' + string(index) + '.mat';
    save(filename, 'OUTPUTgrid');
end