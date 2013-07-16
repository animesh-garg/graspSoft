function pts_np = sphere_samples(n,p,mode)
%% sphere samples 
% generates n samples in dimension p on a unit sphere
%Modified Animesh Garg
% Date: July 2013

if (nargin < 3)
    mode=0; %default mode 
end

%
if mode == 0
    if n > 100
        k = floor( (n/(2*p))^(1/(p-1)) );
    else
        k = n;
    end
    
    if p==2, bases = {linspace(-1,1,k)};
    else [bases{1:p-1}] = ndgrid(linspace(-1,1,k));
    end
    flatcoords = cellfun(@(b) b(:),bases,'UniformOutput',false);
    L = ones(size(flatcoords{1}));
    facepair = [flatcoords{:},L;
                flatcoords{:},-L];
    faces = arrayfun(@(i) facepair(:,[i:p,1:i-1]),(1:p)','UniformOutput',false);
    pts_np = normr(cell2mat(faces));

elseif mode == 1    
    pts_np = normr(randn(n,p));    

elseif mode ==2
    pts_np = randsphere(n,p);

end


end


function X = randsphere(m,n,r)
 
% This function returns an m by n array, X, in which 
% each of the m rows has the n Cartesian coordinates 
% of a random point uniformly-distributed over the 
% interior of an n-dimensional hypersphere with 
% radius r and center at the origin.  The function 
% 'randn' is initially used to generate m sets of n 
% random variables with independent multivariate 
% normal distribution, with mean 0 and variance 1.
% Then the incomplete gamma function, 'gammainc', 
% is used to map these points radially to fit in the 
% hypersphere of finite radius r with a uniform % spatial distribution.
% Roger Stafford - 12/23/05
 
if (nargin < 3)
    r=1; %default unit radius
end

X = randn(m,n);
s2 = sum(X.^2,2);
X = X.*repmat(r*(gammainc(s2/2,n/2).^(1/n))./sqrt(s2),1,n);
