function [valid] = istensors(tensors)

    valid = (size(tensors, 2) == 9);

end