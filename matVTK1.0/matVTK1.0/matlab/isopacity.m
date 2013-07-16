function [valid] = isopacity(colors)

valid = (isnumeric(colors) && size(colors, 2) == 2);

end