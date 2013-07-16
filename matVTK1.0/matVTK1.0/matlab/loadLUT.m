function [color opacity] = loadLUT(lutName)

  load LUTlist
  color = [];
  opacity = [];
  
  if(~isfield(LUTlist, lutName))
     
      return;
  end
  
  color = LUTlist.(lutName).colorLUT;
  opacity = LUTlist.(lutName).opacityLUT;

end