#pragma once
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <Dwmapi.h>
#include <thread>
#include <vector>
#include "lib/pButton.hpp"
#include "lib/pItem.hpp"
#include "lib/pList.hpp"
#include "lib/pMouse.hpp"
#include "lib/pNotification.hpp"
#pragma comment (lib, "Dwmapi.lib")

void setTransparent(HWND handle)
{
	MARGINS margins;
	margins.cxLeftWidth = -1;
	LONG cur_style = GetWindowLong(handle, GWL_EXSTYLE);

	DwmExtendFrameIntoClientArea(handle, &margins);
	SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetWindowLong(handle, GWL_EXSTYLE, cur_style | WS_EX_TRANSPARENT | WS_EX_LAYERED);
}

struct GameInfo
{
	HWND hwnd;
	RECT rect;
	LPCSTR game;
	sf::Vector2i pos;
	sf::Vector2u size;

	GameInfo() {}

	GameInfo(LPCSTR gameW)
	{
		game = gameW;
		hwnd = FindWindowA(0, gameW);
		GetWindowRect(hwnd, &rect);

		pos = this->getPos();
		size = this->getSize();
	}

	void update()
	{
		hwnd = FindWindowA(0, game);
		GetWindowRect(hwnd, &rect);
		pos = this->getPos();
		size = this->getSize();
	}

	sf::Vector2i getPos()
	{
		sf::Vector2i value;

		value.x = rect.left;
		value.y = rect.top;

		return value;
	}

	sf::Vector2u getSize()
	{
		sf::Vector2u value;

		value.x = rect.right - rect.left;
		value.y = rect.bottom - rect.top;

		return value;
	}
};

class pOverlay
{
private:
	sf::RenderWindow Window;
	sf::Font		 Font;
	GameInfo		 gameInfo;
	pMouse			 mouse;

public:
	pList			 list;
	pList			 listSettings;
	pNotification    notification;

	pOverlay(){}

	void createOverlay(LPCSTR Name)
	{
		gameInfo = GameInfo(Name); // Getting game Info
		Font.loadFromFile("Settings/font.ttf");

		Window.create(sf::VideoMode(gameInfo.getSize().x - 1, gameInfo.getSize().y + 1), "pHake 4.0", sf::Style::None); // creating a window in the game's size
		Window.setFramerateLimit(60);
		setTransparent(Window.getSystemHandle()); // making the window transparent & not clickable

		notification.create(&Window);

		mouse.create(&Window);
		mouse.toggle();

		list.create(&Window);
		list.setPostion(Window.getSize().x / 2, Window.getSize().y / 4);
		list.toggle();

		listSettings.create(&Window);
		listSettings.setPostion(list.getPosition().x + listSettings.getSize().x + 5, list.getPosition().y);
		listSettings.toggle();
	}

	void toggle()
	{
		list.toggle();
		listSettings.toggle();
		mouse.toggle();

		if (list.isActive())
			sf::Mouse::setPosition(sf::Vector2i(list.getPosition().x + gameInfo.getPos().x, list.getPosition().y + gameInfo.getPos().y));
	}

	void loop()
	{
		while (Window.isOpen() && gameInfo.hwnd != 0)
		{
			gameInfo.update(); // updating the games window position and size
			if (Window.getPosition() != gameInfo.getPos() || Window.getSize() != gameInfo.getSize())
			{
				Window.setPosition(gameInfo.pos);
				Window.setSize(sf::Vector2u(gameInfo.size.x, gameInfo.size.y));
			}

			sf::Event event;
			while (Window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					Window.close();

				if (event.type == sf::Event::Resized)
				{
					sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
					Window.setView(sf::View(visibleArea));
				}
			}

			list.loop();
			listSettings.loop();
			mouse.Loop();

			Window.clear(sf::Color::Color(0, 0, 0, 0));

			list.draw();
			listSettings.draw();
			mouse.draw();
			notification.draw();

			Window.display();
		}
	}
};