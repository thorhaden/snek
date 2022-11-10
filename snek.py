'''Snake game POC'''

import time
import random
import pygame

SCREEN_SIZE_X = 128
SCREEN_SIZE_Y = 64
PIXELS_PER_SECOND = 8  # Speed


def newfood(screen):
    '''Create and draw a new food.'''
    food_x = random.randrange(SCREEN_SIZE_X / 2) * 2
    food_y = random.randrange(SCREEN_SIZE_Y / 2) * 2
    # pygame.Surface.set_at(screen, (food_x, food_y), 'white')
    pygame.draw.rect(screen, 'white', [food_x, food_y, 2, 2])
    pygame.display.flip()
    print(f'new food at {food_x}, {food_y}')
    return({'x': food_x, 'y': food_y})


def eatfood(screen, food):
    '''Remove an eaten food.'''
    # pygame.Surface.set_at(screen, (food['x'], food['y']), 'black')
    pygame.draw.rect(screen, 'black', [food['x'], food['y'], 2, 2])
    pygame.display.flip()


def move_snake(snake_direction, snake_head, snake_tail):
    '''Move coordinates of snake one step.'''
    snake_tail.append(snake_head)
    xpos = snake_head['x']
    ypos = snake_head['y']
    if snake_direction == 'left':
        xpos -= 2
    elif snake_direction == 'right':
        xpos += 2
    elif snake_direction == 'up':
        ypos -= 2
    elif snake_direction == 'down':
        ypos += 2
    return({'x': xpos, 'y': ypos}, snake_tail)


def draw_snake(screen, snake_head, snake_tail, snake_length):
    '''Draw the snake and it's tail.'''
    if len(snake_tail) > snake_length:  # Remove previously drawn tail
        removepixel = snake_tail.pop(0)
        # pygame.Surface.set_at(screen,
        #                       (removepixel['x'], removepixel['y']),
        #                       'black')
        pygame.draw.rect(screen, 'black', [removepixel['x'], removepixel['y'], 2, 2])
    # pygame.Surface.set_at(screen, (snake_head['x'], snake_head['y']), 'white')
    pygame.draw.rect(screen, 'gray', [snake_head['x'], snake_head['y'], 2, 2])
    for pixel in snake_tail:
        # pygame.Surface.set_at(screen, (pixel['x'], pixel['y']), 'white')
        pygame.draw.rect(screen, 'gray', [pixel['x'], pixel['y'], 2, 2])
    pygame.display.flip()


def main():
    '''Hello, this is main.'''
    pygame.init()
    screen = pygame.display.set_mode([SCREEN_SIZE_X, SCREEN_SIZE_Y])
    pygame.display.set_caption('Snek')

    score = 0

    snake_head = {'x': 64, 'y': 32}
    snake_tail = []
    snake_length = 5
    snake_direction = random.choice(['left', 'right', 'up', 'down'])
    draw_snake(screen, snake_head, snake_tail, snake_length)

    food = newfood(screen)
    if food in snake_tail or food == snake_head:  # Don't put food in the snake
        food = newfood(screen)

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    if snake_direction == 'right':  # don't allow 180 turns
                        continue
                    snake_direction = 'left'
                elif event.key == pygame.K_RIGHT:
                    if snake_direction == 'left':  # don't allow 180 turns
                        continue
                    snake_direction = 'right'
                elif event.key == pygame.K_UP:
                    if snake_direction == 'down':  # don't allow 180 turns
                        continue
                    snake_direction = 'up'
                elif event.key == pygame.K_DOWN:
                    if snake_direction == 'up':  # don't allow 180 turns
                        continue
                    snake_direction = 'down'

        snake_head, snake_tail = move_snake(snake_direction,
                                            snake_head,
                                            snake_tail)

        if snake_head['x'] < 0 or \
           snake_head['y'] < 0 or \
           snake_head['x'] >= SCREEN_SIZE_X or \
           snake_head['y'] >= SCREEN_SIZE_Y:
            print('Wallcrash!')
            running = False

        if snake_head in snake_tail:
            print("Selfcrash!")
            running = False

        if snake_head['x'] == food['x'] and snake_head['y'] == food['y']:
            eatfood(screen, food)
            snake_length += 5
            score += 1
            print(f"Food nomz! Score: {score}")
            food = newfood(screen)
            if food in snake_tail or food == snake_head:  # Don't put food in the snake
                food = newfood(screen)

        draw_snake(screen, snake_head, snake_tail, snake_length)

        time.sleep(1/PIXELS_PER_SECOND)

    print(f'GAME OVER - {score} points.')
    pygame.quit()


if __name__ == "__main__":
    main()
