function [valid] = ismatvtkhandle(candidate)

type = isa(candidate,'uint16');
dimsok = (ndims(candidate) == 2);
count = (numel(candidate) == 1);

valid = (type && dimsok && count);

end