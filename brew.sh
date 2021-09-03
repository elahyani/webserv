which -s brew
		if [[ $? != 0 ]] ; then
			echo "Brew not installed, installling..."
			# Install brew
			rm -rf $HOME/.brew
			git clone --depth=1 https://github.com/Homebrew/brew $HOME/.brew
			echo 'export PATH=$HOME/.brew/bin:$PATH' >> $HOME/.zshrc
			source $HOME/.zshrc
		fi
