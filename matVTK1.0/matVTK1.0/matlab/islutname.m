function [valid] = islutname(name)

    valid = 0;
    [a b] = loadLUT(name);
    if(~isempty(a))
        valid = 1;
    end
end
