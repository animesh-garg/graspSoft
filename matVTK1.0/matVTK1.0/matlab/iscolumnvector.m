function [valid] = isvector(vec)

    valid = (ndims(vec) == 2 && size(vec, 2) == 1);

end