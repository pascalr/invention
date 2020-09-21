https://www.tutorialspoint.com/ruby-on-rails/rails-installation.htm

# INSTALL rvm (ruby version manager)
# instructions from https://github.com/rvm/ubuntu_rvm
sudo apt-get install software-properties-common

sudo apt-add-repository -y ppa:rael-gc/rvm
sudo apt-get update
sudo apt-get install rvm

echo 'source "/etc/profile.d/rvm.sh"' >> ~/.bashrc
sudo reboot now
rvm install ruby

# INSTALL sqlite3

sudo apt install libsqlite3-dev sqlite3

# INSTALL node
# install nvm
# follow instructions at https://github.com/creationix/nvm
# reload terminal
nvm install node

# INSTALL yarn
curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | sudo apt-key add -
echo "deb https://dl.yarnpkg.com/debian/ stable main" | sudo tee /etc/apt/sources.list.d/yarn.list
sudo apt update && sudo apt install yarn

# https://guides.rubyonrails.org/getting_started.html
# rails needs ruby, sqlite3, node.js, yarn

gem install rails

bundle install






install ruby with rvm
sudo apt-get install zlib1g-dev libssl-dev liblzma-dev
\curl -sSL https://get.rvm.io | bash -s stable --ruby
rvm list known

Download ruby and compile from source
https://www.ruby-lang.org/en/downloads/

# sudo apt install ruby-dev not usefulll probably since installing from source

install yarn for rails
curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | sudo apt-key add -
echo "deb https://dl.yarnpkg.com/debian/ stable main" | sudo tee /etc/apt/sources.list.d/yarn.list
sudo apt update && sudo apt install yarn


sudo gem install rails
(cd client && bundle install)
