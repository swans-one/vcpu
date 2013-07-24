# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  # All Vagrant configuration is done here. The most common configuration
  # options are documented and commented below. For a complete reference,
  # please see the online documentation at vagrantup.com.

  # The box we're using
  config.vm.box = "debian-wheezy-70rc1-x64"
  # And the url to dowload it from if it's not already on the computer
  config.vm.box_url = "http://puppet-vagrant-boxes.puppetlabs.com/debian-70rc1-x64-vbox4210.box"

  # We're going to use puppet to provision this box.
  config.vm.provision :puppet do |puppet|
    puppet.module_path = "~/PuppetModules"
    puppet.manifests_path = "manifests"
    puppet.manifest_file  = "entry.pp"
  end

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine. In the example below,
  # accessing "localhost:8080" will access port 80 on the guest machine.
  config.vm.network :forwarded_port, guest: 80, host: 8765

end
