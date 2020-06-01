RPM = 60;
microstepping = 8;

nbStepParTour = 200 * 8;
delayStep = 1 / RPM * 60 / nbStepParTour * 1000 # temps par tour divise par le nombre de step par tour

disp('Le delai par step est de (ms):')
disp(delayStep)

# w [omega]: La vitesse de deplacement de l'axe (rad/s)
# qui equivaut a RPM moteur * 2*pi / 60 * ratio engrenage (5)
ratio = 5;
w = RPM * 2*3.1416/60/ratio;
r = 12*25.4; # mm
thetas = [0:0.01:3.1416];
v = r*w*sin(thetas);
distance_x = 32*25.4;

# objectif, accuracy = 0.2mm
precision = 2*3.1416/nbStepParTour*r/ratio;
disp('La precision est de (mm):')
disp(precision)

figure
hold on
plot(r*cos(thetas),r*sin(thetas))
xlabel('x')
ylabel('z')
axis([-r  r 0 r])
title('Position des points')

disp('Max speed (mm/s):')
disp(max(v))
disp('A cette vitesse, le trajet en x serait parcouru en (s):')
disp(distance_x/max(v))
disp('Pour compenser cette vitesse, le moteur en x devrait tourner a (RPM):')
disp(max(v)/(2*3.1416*0.625*25.4)*60)

disp('To do one 180 degrees turn on the theta axis, it will take (s):')
disp(2*3.1416/w)