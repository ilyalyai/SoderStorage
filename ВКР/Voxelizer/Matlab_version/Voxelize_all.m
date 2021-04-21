dirData = dir('75/*.stl');
for index = 1:58
    filename1 = convertStringsToChars(string(dirData(index).folder) + '\' + string(dirData(index).name));
    [OUTPUTgrid] = VOXELISE(32,32,32,filename1,'xyz');
    OUTPUTgrid = int8(OUTPUTgrid);
    filename = '75_mat/' + string(index) + '.mat';
    save(filename, 'OUTPUTgrid');
end