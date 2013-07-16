function [valid] = ispoints(points)

    valid = (size(points, 2) == 3);
end