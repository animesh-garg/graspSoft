function [valid] = iscolors(colors)

valid = (isnumeric(colors) && size(colors, 2) == 4);

end