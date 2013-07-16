function [valid] = isvolume(v)

valid = (length(size(v)) == 3);

end