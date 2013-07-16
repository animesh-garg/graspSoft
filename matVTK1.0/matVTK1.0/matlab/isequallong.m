function [valid] = isequallong(left, right)

    valid = (size(left,1) == size(right,1));
end