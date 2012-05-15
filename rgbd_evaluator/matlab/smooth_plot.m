function smooth_plot( gauss_sigma, X, Y, varargin )
%SMOOTH_PLOT plots a smooth function for the given unordered x/y pairs

  %clf;hold on;

  [ x_smooth, y_smooth ] = smooth_data( X, Y, gauss_sigma );
  plot( x_smooth, y_smooth, varargin{:} );
  h=plot( X, Y, varargin{:}, 'LineStyle', 'none', 'Marker','+' );
  hasbehavior(h,'legend',false);

end

